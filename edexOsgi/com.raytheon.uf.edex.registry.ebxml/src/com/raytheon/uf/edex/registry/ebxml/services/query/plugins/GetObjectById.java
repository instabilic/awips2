/**
 * This software was developed and / or modified by Raytheon Company,
 * pursuant to Contract DG133W-05-CQ-1067 with the US Government.
 * 
 * U.S. EXPORT CONTROLLED TECHNICAL DATA
 * This software product contains export-restricted data whose
 * export/transfer/disclosure is restricted by U.S. law. Dissemination
 * to non-U.S. persons whether in the United States or abroad requires
 * an export license or other authorization.
 * 
 * Contractor Name:        Raytheon Company
 * Contractor Address:     6825 Pine Street, Suite 340
 *                         Mail Stop B8
 *                         Omaha, NE 68106
 *                         402.291.0100
 * 
 * See the AWIPS II Master Rights File ("Master Rights File.pdf") for
 * further licensing information.
 **/
package com.raytheon.uf.edex.registry.ebxml.services.query.plugins;

import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebResult;

import oasis.names.tc.ebxml.regrep.wsdl.registry.services.v4.MsgRegistryException;
import oasis.names.tc.ebxml.regrep.xsd.query.v4.QueryRequest;
import oasis.names.tc.ebxml.regrep.xsd.query.v4.QueryResponse;
import oasis.names.tc.ebxml.regrep.xsd.rim.v4.QueryType;

import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import com.raytheon.uf.common.registry.schemas.ebxml.util.EbxmlNamespaces;
import com.raytheon.uf.edex.registry.ebxml.dao.RegistryObjectDao;
import com.raytheon.uf.edex.registry.ebxml.services.query.QueryConstants;
import com.raytheon.uf.edex.registry.ebxml.services.query.RegistryQueryUtil;

/**
 * The canonical query GetObjectById allows clients to find RegistryObjects
 * based upon the value of their id attribute.
 * 
 * 
 * 
 * <pre>
 * 
 * SOFTWARE HISTORY
 * 
 * Date         Ticket#    Engineer    Description
 * ------------ ---------- ----------- --------------------------
 * Jan 18, 2012            bphillip     Initial creation
 * 3/18/2013    1802       bphillip    Modified to use transaction boundaries and spring dao injection
 * 4/9/2013     1802       bphillip     Changed abstract method signature, modified return processing, and changed static variables
 * 4/19/2013    1931       bphillip    Fixed null pointer issue
 * 10/8/2013    1682       bphillip    Refactored querying
 * </pre>
 * 
 * @author bphillip
 * @version 1.0
 */
public class GetObjectById extends RegistryQueryPlugin {

    private RegistryObjectDao registryObjectDao;

    @Override
    @WebMethod(action = EXECUTE_QUERY_ACTION)
    @WebResult(name = "QueryResponse", targetNamespace = EbxmlNamespaces.QUERY_URI, partName = "partQueryResponse")
    @Transactional(propagation = Propagation.MANDATORY, readOnly = true)
    public QueryResponse executeQuery(
            @WebParam(name = "QueryRequest", targetNamespace = EbxmlNamespaces.QUERY_URI, partName = "partQueryRequest") QueryRequest queryRequest)
            throws MsgRegistryException {
        QueryResponse response = new QueryResponse();
        QueryType queryType = queryRequest.getQuery();
        String id = queryType.getSlotValue(QueryConstants.ID);

        if (id.contains("?") || id.contains("%")) {
            response.getRegistryObjects().addAll(
                    registryObjectDao.getByIdUsingLike(RegistryQueryUtil
                            .substituteWildcardCharacters(id)));
        } else {
            response.getRegistryObjects().add(registryObjectDao.getById(id));
        }
        return response;
    }

    public void setRegistryObjectDao(RegistryObjectDao registryObjectDao) {
        this.registryObjectDao = registryObjectDao;
    }

}
